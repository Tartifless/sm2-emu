//  ____  __  __  ____         _____ __  __ _   _
// / ___||  \/  ||___ \       | ____|  \/  | | | |
// \___ \| |\/| |  __) |_____ |  _| | |\/| | | | |
//  ___) | |  | | / __/|_____|| |___| |  | | |_| |
// |____/|_|  |_||_____|      |_____|_|  |_|\___/
//
// A Sega Model 2 arcade emulator.
// Copyright (c) 2025+ Daniel Martin (dmanlfc)
// SPDX-License-Identifier: BSD-3-Clause
//
// This header must not be removed. The source files in this project may not be
// used to contribute to commercial projects or for monetary gain without the
// express written permission of the author.
//
// The answer of the drive board of Indy 500, Touring Car, Over Rev and Super
// GT 24h (EPR-18261), which the game reads on the I/O controller's port D. The
// board replies to the command it was last sent: $80 and $83 with the wheel's
// position on its ADC, $81 with $FF, $82 with its DIP switches 1, $84 with
// its centre, and any other command by echoing it.
//
// Indy 500's VOLUME SETTING asks $80 when the operator saves the calibration
// and keeps the answer as the board's centre, which it sends back as $9x/$Cx
// at every boot: without this answer the centre became $FF, and the board's
// springs pushed the wheel hard right.
#pragma once

#include "core/types.h"

namespace sm2::hw {

struct IndyDriveReply {
    u8 command = 0x00;
    u8 centre  = 0x80;  ///< $80 at power-up; $9x sets its high nibble, $Cx its low.

    /// A byte the game wrote to the board.
    void write(u8 value)
    {
        command = value;
        if ((value >> 4) == 0x9) {
            centre = static_cast<u8>((centre & 0x0f) | (value << 4));
        } else if ((value >> 4) == 0xc) {
            centre = static_cast<u8>((centre & 0xf0) | (value & 0x0f));
        }
    }

    /// The answer, the wheel on the board's ADC being `wheel`: the steering
    /// pot the game reads too.
    [[nodiscard]] u8 read(u8 wheel) const
    {
        switch (command) {
            case 0x80:
            case 0x83: return wheel;
            case 0x81: return 0xff;
            case 0x82: return 0xff;  // DIP switches 1, all OFF
            case 0x84: return centre;
            default:   return command;
        }
    }
};

}  // namespace sm2::hw
