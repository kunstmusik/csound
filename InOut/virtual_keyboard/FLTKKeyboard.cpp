/*
    FLTKKeyboard.cpp:

    Copyright (C) 2006 Steven Yi

    This file is part of Csound.

    The Csound Library is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    Csound is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with Csound; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

#include <iostream>

#include <FL/fl_draw.H>
#include "FLTKKeyboard.hpp"

FLTKKeyboard::FLTKKeyboard(CSOUND *csound, int X, int Y, int W, int H, const char *L)
	: Fl_Widget(X, Y, W, H, L) {

    this->csound = csound;
    this->mutex = csound->Create_Mutex(0);

  	FLTKKeyboard *o = this;

	o->box(FL_FLAT_BOX);
	o->color(FL_BACKGROUND_COLOR);
	o->selection_color(FL_BACKGROUND_COLOR);
	o->labeltype(FL_NO_LABEL);
	o->labelfont(0);
	o->labelsize(14);
	o->labelcolor(FL_FOREGROUND_COLOR);
	o->user_data((void*)(this));
	o->align(FL_ALIGN_TOP);
	o->when(FL_WHEN_RELEASE);

	for(int i = 0; i < 88; i++) {
		keyStates[i] = 0;
		changedKeyStates[i] = 0;
	}

	lastMidiKey = -1;

	whiteKeys[0] = 0;
	whiteKeys[1] = 2;
	whiteKeys[2] = 4;
	whiteKeys[3] = 5;
	whiteKeys[4] = 7;
	whiteKeys[5] = 9;
	whiteKeys[6] = 11;

	octave = 5;

	aNotesOff = 0;
}

FLTKKeyboard::~FLTKKeyboard() {
	if (mutex) {
      	csound->DestroyMutex(mutex);
      	mutex = (void*) 0;
    }
}

void FLTKKeyboard::lock() {
	if(mutex) {
      	csound->LockMutex(mutex);
	}
}

void FLTKKeyboard::unlock() {
	if(mutex) {
      	csound->UnlockMutex(mutex);
	}
}

void FLTKKeyboard::allNotesOff() {
	this->lock();

	for(int i = 0; i < 88; i++) {
		this->keyStates[i] = -1;
	}

	this->lastMidiKey = -1;

	this->aNotesOff = 1;

	this->unlock();

	this->redraw();
}

int FLTKKeyboard::getMidiValForWhiteKey(int whiteKeyNum) {
    if(whiteKeyNum < 2) {
        return whiteKeyNum * 2;
    }

    int adjusted = whiteKeyNum - 2;

    int oct = adjusted / 7;
    int key = adjusted % 7;

    return 3 + (oct * 12) + whiteKeys[key];
}

int FLTKKeyboard::getMIDIKey(int x, int y) {
	if(x > this->w()) {
		return 87;
	}

	if(x < 0) {
		return 0;
	}

    // 52 white keys
    int whiteKey = x  / whiteKeyWidth;

    int extra = x % whiteKeyWidth;

    if(whiteKey < 2) {
        if(whiteKey == 0) {
            if(y > blackKeyHeight) {
                return whiteKey;
            } else {
                if(extra > rightKeyBound) {
                    return whiteKey + 1;
                }
                return whiteKey;
            }
        } else {
            if(y > blackKeyHeight) {
                return getMidiValForWhiteKey(whiteKey);
            } else {
                if(extra < leftKeyBound) {
                    return getMidiValForWhiteKey(whiteKey) - 1;
                }
                return getMidiValForWhiteKey(whiteKey);
            }
        }
    }

    int adjustedKey = (whiteKey - 2) % 7;

    if(adjustedKey == 0 || adjustedKey == 3) {

        if(y > blackKeyHeight) {
            return getMidiValForWhiteKey(whiteKey);
        } else {
            if(extra > rightKeyBound) {
                return getMidiValForWhiteKey(whiteKey) + 1;
            }
            return getMidiValForWhiteKey(whiteKey);
        }
    } else if(adjustedKey == 2 || adjustedKey == 6) {
        if(y > blackKeyHeight) {
            return getMidiValForWhiteKey(whiteKey);
        } else {
            if(extra < leftKeyBound) {
                return getMidiValForWhiteKey(whiteKey) - 1;
            }
            return getMidiValForWhiteKey(whiteKey);
        }
    }

    if(y > blackKeyHeight) {
        return getMidiValForWhiteKey(whiteKey);
    }

    if(extra < leftKeyBound) {
        return getMidiValForWhiteKey(whiteKey) - 1;
    }

    if(extra > rightKeyBound) {
        return getMidiValForWhiteKey(whiteKey) + 1;
    }

    return getMidiValForWhiteKey(whiteKey);

}

void FLTKKeyboard::handleKey(int key, int value) {
	int index = -1;

	switch(key) {
		case 'z':
		    index = 0;
		    break;
		case 's':
		    index = 1;
		    break;
		case 'x':
		    index = 2;
		    break;
		case 'd':
		    index = 3;
		    break;
		case 'c':
		    index = 4;
		    break;
		case 'v':
		    index = 5;
		    break;
		case 'g':
		    index = 6;
		    break;
		case 'b':
		    index = 7;
		    break;
		case 'h':
		    index = 8;
		    break;
		case 'n':
		    index = 9;
		    break;
		case 'j':
		    index = 10;
		    break;
		case 'm':
		    index = 11;
		    break;
		case 'q':
		    index = 12;
		    break;
		case '2':
		    index = 13;
		    break;
		case 'w':
		    index = 14;
		    break;
		case '3':
		    index = 15;
		    break;
		case 'e':
		    index = 16;
		    break;
		case 'r':
		    index = 17;
		    break;
		case '5':
		    index = 18;
		    break;
		case 't':
		    index = 19;
		    break;
		case '6':
		    index = 20;
		    break;
		case 'y':
		    index = 21;
		    break;
		case '7':
		    index = 22;
		    break;
		case 'u':
		    index = 23;
		    break;
		case 'i':
		    index = 24;
		    break;
		case '9':
		    index = 25;
		    break;
		case 'o':
		    index = 26;
		    break;
		case '0':
		    index = 27;
		    break;
		case 'p':
		    index = 28;
		    break;
		default:
			return;
	}

	if(index < 0) {
		return;
	}

	index = ((octave * 12) + index) - 21;

	if(index < 0 || index > 87) {
		return;
	}

	lock();
	if(keyStates[index] != value) {
		keyStates[index] = value;
	}
	unlock();
}

int FLTKKeyboard::handle(int event) {

    int key;

    switch(event) {
        case FL_PUSH:
			if(Fl::event_button2() || Fl::event_button3()) {
				return 1;
			}

            key = getMIDIKey(Fl::event_x(), Fl::event_y());

			this->lock();

            lastMidiKey = key;
            keyStates[key] = 1;

            this->unlock();

            this->redraw();

            return 1;
        case FL_DRAG:
			if(Fl::event_button2() || Fl::event_button3()) {
				return 1;
			}
            key = getMIDIKey(Fl::event_x(), Fl::event_y());

            if(key != lastMidiKey) {

            	this->lock();

                keyStates[lastMidiKey] = -1;

                if(keyStates[key] != 1) {
                	keyStates[key] = 1;
                }

                lastMidiKey = key;

                this->unlock();

                this->redraw();

            }
            return 1;
        case FL_RELEASE:
        	if(Fl::event_button1()) {
				return 1;
			}

			key = getMIDIKey(Fl::event_x(), Fl::event_y());

			this->lock();

            keyStates[key] = 0;

			if(lastMidiKey >= -1) {
            	keyStates[lastMidiKey] = -1;
			}

            lastMidiKey = -1;

            this->unlock();

            this->redraw();

            return 1;
        case FL_MOVE:
        	if(lastMidiKey >= 0) {

				this->lock();
            	keyStates[lastMidiKey] = 0;
            	lastMidiKey = -1;
            	this->unlock();

        	}
        	return 1;
    	case FL_KEYDOWN:
			//csound->Message(csound, "Key Down: Code: %d\n", Fl::event_key());
			handleKey(Fl::event_key(), 1);
			this->redraw();
			return 1;
    	case FL_KEYUP:
    		//csound->Message(csound, "Key Up: Code: %d\n", Fl::event_key());
    		handleKey(Fl::event_key(), -1);
    		this->redraw();
        	return 1;
        default:
            return Fl_Widget::handle(event);
    }

}

int FLTKKeyboard::isWhiteKey(int key) {
    if(key < 3) {
        return !(key % 2);
    }

    int adjustedKey = (key - 3) % 12;

    switch(adjustedKey) {
        case 0:
        case 2:
        case 4:
        case 5:
        case 7:
        case 9:
        case 11:
            return 1;
    }
    return 0;


}

void FLTKKeyboard::draw() {
    int width = whiteKeyWidth;

    int runningX = 0;
    int yval = this->y();

    // Draw White Keys
    for(int i = 0; i < 88; i++) {
        if(isWhiteKey(i)) {
            if(keyStates[i] == 1) {
                fl_draw_box(box(), runningX, yval, width, whiteKeyHeight, FL_BLUE);
            } else {
                fl_draw_box(box(), runningX, yval, width, whiteKeyHeight, FL_WHITE);
            }
            fl_rect(runningX, yval, width, whiteKeyHeight, FL_BLACK);
            runningX += width;
        }
    }

    runningX = 0;

    // Draw Black Keys
    for(int i = 0; i < 88; i++) {
        if(isWhiteKey(i)) {
            runningX += width;
        } else {
            if(keyStates[i] == 1) {
                fl_draw_box(box(), runningX - 5, yval, blackKeyWidth, blackKeyHeight, FL_BLUE);
            } else {
                fl_draw_box(box(), runningX - 5, yval, blackKeyWidth, blackKeyHeight, FL_BLACK);
            }
        }
    }
}
