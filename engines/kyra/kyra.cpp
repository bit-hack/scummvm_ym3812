/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include "common/config-manager.h"

#include "sound/mididrv.h"
#include "sound/mixer.h"

#include "kyra/kyra.h"
#include "kyra/sound.h"
#include "kyra/resource.h"
#include "kyra/screen.h"
#include "kyra/text.h"
#include "kyra/timer.h"
#include "kyra/script.h"

namespace Kyra {

KyraEngine::KyraEngine(OSystem *system, const GameFlags &flags)
	: Engine(system) {
	_res = 0;
	_sound = 0;
	_text = 0;
	_staticres = 0;
	_timer = 0;
	_scriptInterpreter = 0;

	_flags = flags;
	_gameSpeed = 60;
	_tickLength = (uint8)(1000.0 / _gameSpeed);

	_quitFlag = false;

	_skipFlag = false;

	_trackMap = 0;
	_trackMapSize = 0;
	_lastMusicCommand = -1;
	_curSfxFile = _curMusicTheme = -1;

	memset(_flagsTable, 0, sizeof(_flagsTable));

	// sets up all engine specific debug levels
	Common::addSpecialDebugLevel(kDebugLevelScriptFuncs, "ScriptFuncs", "Script function debug level");
	Common::addSpecialDebugLevel(kDebugLevelScript, "Script", "Script interpreter debug level");
	Common::addSpecialDebugLevel(kDebugLevelSprites, "Sprites", "Sprite debug level");
	Common::addSpecialDebugLevel(kDebugLevelScreen, "Screen", "Screen debug level");
	Common::addSpecialDebugLevel(kDebugLevelSound, "Sound", "Sound debug level");
	Common::addSpecialDebugLevel(kDebugLevelAnimator, "Animator", "Animator debug level");
	Common::addSpecialDebugLevel(kDebugLevelMain, "Main", "Generic debug level");
	Common::addSpecialDebugLevel(kDebugLevelGUI, "GUI", "GUI debug level");
	Common::addSpecialDebugLevel(kDebugLevelSequence, "Sequence", "Sequence debug level");
	Common::addSpecialDebugLevel(kDebugLevelMovie, "Movie", "Movie debug level");
	Common::addSpecialDebugLevel(kDebugLevelTimer, "Timer", "Timer debug level");

	system->getEventManager()->registerRandomSource(_rnd, "kyra");
}

int KyraEngine::init() {
	registerDefaultSettings();

	// Setup mixer
	if (!_mixer->isReady())
		warning("Sound initialization failed.");

	_mixer->setVolumeForSoundType(Audio::Mixer::kSFXSoundType, ConfMan.getInt("sfx_volume"));
	_mixer->setVolumeForSoundType(Audio::Mixer::kMusicSoundType, ConfMan.getInt("music_volume"));
	_mixer->setVolumeForSoundType(Audio::Mixer::kSpeechSoundType, ConfMan.getInt("speech_volume"));

	// We prefer AdLib over native MIDI, since our AdLib playback code is much
	// more mature than our MIDI player. For example we are missing MT-32 support
	// and it seems our MIDI playback code has threading issues (see bug #1506583
	// "KYRA1: Crash on exceeded polyphony" for more information).
	int midiDriver = MidiDriver::detectMusicDriver(MDT_MIDI | MDT_ADLIB/* | MDT_PREFER_MIDI*/);

	if (_flags.platform == Common::kPlatformFMTowns || _flags.platform == Common::kPlatformPC98) {
		// TODO: currently we don't support the PC98 sound data,
		// but since it has the FM-Towns data files, we just use the
		// FM-Towns driver
		if (_flags.gameID == GI_KYRA1)
			_sound = new SoundTowns(this, _mixer);
		else
			_sound = new SoundTowns_v2(this, _mixer);
	} else if (midiDriver == MD_ADLIB) {
		_sound = new SoundAdlibPC(this, _mixer);
		assert(_sound);
	} else {
		bool native_mt32 = ((midiDriver == MD_MT32) || ConfMan.getBool("native_mt32"));

		MidiDriver *driver = MidiDriver::createMidi(midiDriver);
		assert(driver);
		if (native_mt32)
			driver->property(MidiDriver::PROP_CHANNEL_MASK, 0x03FE);

		SoundMidiPC *soundMidiPc = new SoundMidiPC(this, _mixer, driver);
		_sound = soundMidiPc;
		assert(_sound);
		soundMidiPc->hasNativeMT32(native_mt32);

		// Unlike some SCUMM games, it's not that the MIDI sounds are
		// missing. It's just that at least at the time of writing they
		// are decidedly inferior to the Adlib ones.
		if (ConfMan.getBool("multi_midi")) {
			SoundAdlibPC *adlib = new SoundAdlibPC(this, _mixer);
			assert(adlib);

			_sound = new MixedSoundDriver(this, _mixer, soundMidiPc, adlib);
			assert(_sound);
		}
	}

	_res = new Resource(this);
	assert(_res);
	_res->reset();
	_staticres = new StaticResource(this);
	assert(_staticres);
	if (!_staticres->init())
		error("_staticres->init() failed");
	_timer = new TimerManager(this, _system);
	assert(_timer);
	setupTimers();
	_scriptInterpreter = new ScriptHelper(this);
	assert(_scriptInterpreter);

	setupOpcodeTable();
	readSettings();

	_lang = 0;
	Common::Language lang = Common::parseLanguage(ConfMan.get("language"));

	if (_flags.gameID == GI_KYRA2 || _flags.gameID == GI_KYRA3) {
		switch (lang) {
		case Common::EN_ANY:
		case Common::EN_USA:
		case Common::EN_GRB:
			_lang = 0;
			break;

		case Common::FR_FRA:
			_lang = 1;
			break;

		case Common::DE_DEU:
			_lang = 2;
			break;

		case Common::JA_JPN:
			_lang = 3;
			break;

		default:
			warning("unsupported language, switching back to English");
			_lang = 0;
			break;
		}
	}

	return 0;
}

KyraEngine::~KyraEngine() {
	delete _res;
	delete _sound;
	delete _text;
	delete _timer;
	delete _scriptInterpreter;
}

void KyraEngine::quitGame() {
	debugC(9, kDebugLevelMain, "KyraEngine::quitGame()");
	_quitFlag = true;
	// Nothing to do here
}

Common::Point KyraEngine::getMousePos() const {
	Common::Point mouse = _eventMan->getMousePos();

	if (_flags.useHiResOverlay) {
		mouse.x >>= 1;
		mouse.y >>= 1;
	}

	return mouse;
}

int KyraEngine::setGameFlag(int flag) {
	_flagsTable[flag >> 3] |= (1 << (flag & 7));
	return 1;
}

int KyraEngine::queryGameFlag(int flag) const {
	return ((_flagsTable[flag >> 3] >> (flag & 7)) & 1);
}

int KyraEngine::resetGameFlag(int flag) {
	_flagsTable[flag >> 3] &= ~(1 << (flag & 7));
	return 0;
}

void KyraEngine::delayUntil(uint32 timestamp, bool updateTimers, bool update, bool isMainLoop) {
	while (_system->getMillis() < timestamp && !_quitFlag) {
		if (timestamp - _system->getMillis() >= 10)
			delay(10, update, isMainLoop);
	}
}

void KyraEngine::delay(uint32 amount, bool update, bool isMainLoop) {
	_system->delayMillis(amount);
}

void KyraEngine::delayWithTicks(int ticks) {
	delay(ticks * _tickLength);
}

void KyraEngine::registerDefaultSettings() {
	if (_flags.gameID != GI_KYRA3)
		ConfMan.registerDefault("cdaudio", (_flags.platform == Common::kPlatformFMTowns || _flags.platform == Common::kPlatformPC98));
}

void KyraEngine::readSettings() {
	_configWalkspeed = ConfMan.getInt("walkspeed");
	_configMusic = ConfMan.getBool("music_mute") ? 0 : ((ConfMan.getBool("cdaudio") && (_flags.platform == Common::kPlatformFMTowns || _flags.platform == Common::kPlatformPC98)) ? 2 : 1);	
	_configSounds = ConfMan.getBool("sfx_mute") ? 0 : 1;

	_sound->enableMusic(_configMusic);
	_sound->enableSFX(_configSounds);

	bool speechMute = ConfMan.getBool("speech_mute");
	bool subtitles = ConfMan.getBool("subtitles");

	if (!speechMute && subtitles)
		_configVoice = 2;	// Voice & Text
	else if (!speechMute && !subtitles)
		_configVoice = 1;	// Voice only
	else
		_configVoice = 0;	// Text only

	setWalkspeed(_configWalkspeed);
}

void KyraEngine::writeSettings() {
	bool speechMute, subtitles;

	ConfMan.setInt("walkspeed", _configWalkspeed);
	ConfMan.setBool("music_mute", _configMusic == 0);
	ConfMan.setBool("cdaudio", _configMusic == 2);
	ConfMan.setBool("sfx_mute", _configSounds == 0);

	switch (_configVoice) {
	case 0:		// Text only
		speechMute = true;
		subtitles = true;
		break;
	case 1:		// Voice only
		speechMute = false;
		subtitles = false;
		break;
	default:	// Voice & Text
		speechMute = false;
		subtitles = true;
		break;
	}

	if (!_configMusic)
		_sound->beginFadeOut();

	_sound->enableMusic(_configMusic);
	_sound->enableSFX(_configSounds);

	ConfMan.setBool("speech_mute", speechMute);
	ConfMan.setBool("subtitles", subtitles);

	ConfMan.flushToDisk();
}

bool KyraEngine::speechEnabled() {
	return _flags.isTalkie && (_configVoice == 1 || _configVoice == 2);
}

bool KyraEngine::textEnabled() {
	return !_flags.isTalkie || (_configVoice == 0 || _configVoice == 2);
}

} // End of namespace Kyra


