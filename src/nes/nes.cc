#include "nes.h"

void Nes::SetByte(const uint16_t addr, const uint8_t byte) {
    if (addr >= 0x5FF8 and addr <= 0x5FFF) { // bank switching
        // TODO: check
        cpu_->bank_switch(addr, byte); 
    } else if (addr >= 0x4000 and addr <= 0x4017) { // APU
        apu_->SetByte(addr, byte);
    } else {
        memory_->SetByte(addr, byte);
    }
}

uint8_t Nes::GetByte(const uint16_t addr) {
    return memory_->GetByte(addr);
}

uint16_t Nes::GetWord(const uint16_t addr) {
    return memory->GetWord(addr);
}



/* struct AudioData { */
/*     float prev = 0; */
/*     int cycles = 0; */
/*     int  */
/*     double play_start; */
/*     bool new_play = true; */
/*     PaStream *stream; */
/* }; */



    /* AudioData *ad = (AudioData *)userData; */

    /* queue<vector<uint8_t>> to_mix; */

    /* double play_gap = ad->apu->cpu()->play_gap(); */

    /* // for(int i = 0; i < framesPerBuffer; i++) { */
    /* //     double wt = Pa_GetStreamTime(ad->stream); */
    /* //     if (wt - ad->play_start > play_gap) { */
    /* //         ad->play_start = wt; */
    /* //         ad->new_play = false; */
    /* //     } */
    /* //     if (!ad->new_play) { */
    /* //         ad->cycles += CYCLES_PER_SAMPLE; */
    /* //         if (ad->apu->cpu()->play_nsf(ad->cycles)) { */
    /* //             ad->new_play = true; */
    /* //         } */
    /* //     } */
    /* //     for(int i = 0; i < CYCLES_PER_SAMPLE; i++) { */
    /* //         ad->apu->CPUClock(); */
    /* //     } */
    /* //     vector<uint8_t> *partial = new vector<uint8_t>; */
    /* //     ad->apu->GetCurrent(*partial); */
    /* //     to_mix.push(*partial); */
    /* // } */

    /* queue<float> mixed; */
    /* APUMixer apu_mixer; */

    /* apu_mixer.Mix(to_mix, SAMPLE_RATE, ad->prev, mixed); */
    /* if(!mixed.empty()) */
    /*     ad->prev = mixed.back(); */


/**
* @brief Called by NesMemory. Captures writes from 6502 addresses 0x5FF8 to
* 0x5FFF (corresponding to 6502 banks 0 - 8. The byte written is which NSF file
* bank should be placed into the 6502 bank.
*
* @param addr
* @param byte
*/
void Cpu::bank_switch(uint16_t addr, uint8_t byte) {
    int bank = (addr & 0x01) % 8;
    uint16_t padding = reader_->data_load_address() & 0x0FFF;
    uint16_t start_address = 0x8000 + padding;
    for(int j = 0; j < (1 << 12); j++) {
        memory_->set_byte(start_address + (bank * 1<<12) + j,
                          reader_->banks(bank, j));
    }
}
