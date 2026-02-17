#pragma once
#include "gap_buff.hpp"

#include "unit_tests.hpp"


namespace spd {
    namespace unit_test {
        inline void GapBufferTests() {
            LOG_I("--- STARTING GAP BUFFER UNIT TESTS ---\n");

            // Test 1: Initialization & Basic Insert
            {
                LOG_I("[Test 1] Basic Insertion\n");
                spd::GapBuffer gb; // Initial cap 8
                gb.Insert('A');
                gb.Insert('B');
                gb.Insert('C');
                
                assert(gb.GetSize() == 3);
                gb.Validate();
                LOG_I("Test 1 Passed.\n");
            }

            // Test 2: Moving the Cursor & Shifting Gap
            {
                LOG_I("[Test 2] Gap Shifting (Cursor Movement)\n");
                spd::GapBuffer gb;
                gb.Insert('H');
                gb.Insert('e');
                gb.Insert('l');
                gb.Insert('l');
                gb.Insert('o'); // Content: Hello[___], Start: 5
                
                gb.MoveCursor(2); // Should move gap between 'e' and 'l'
                // Logic check: m_data should look like [H][e][_][_][_][l][l][o]
                
                gb.Insert('!'); // Insert at index 2
                // Expected: He!llo
                
                assert(gb.GetSize() == 6);
                gb.Validate();
                gb.DumpState();
                LOG_I("Test 2 Passed.\n");
            }

            // Test 3: Triggering Growth (The Stress Test)
            {
                LOG_I("[Test 3] Buffer Growth & Integrity\n");
                spd::GapBuffer gb; // Initial cap 8
                
                for(int i = 0; i < 20; ++i) {
                    gb.Insert('x');
                }
                
                // If this doesn't crash and Validate() passes, 
                // your Grow() logic is successfully relocating the suffix.
                assert(gb.GetSize() == 20);
                gb.Validate();
                LOG_I("Test 3 Passed.\n");
            }

            // Test 4: Backspace and Delete logic
            {
                LOG_I("[Test 4] Backspace vs Delete\n");
                spd::GapBuffer gb;
                gb.Insert('A');
                gb.Insert('B');
                gb.Insert('C');
                
                gb.MoveCursor(1); // Gap at index 1: A[___]BC
                
                gb.Delete();    // Should remove 'B' -> A[____]C
                gb.BackSpace(); // Should remove 'A' -> [_____]C
                
                assert(gb.GetSize() == 1);
                gb.Validate();
                LOG_I("Test 4 Passed.\n");
            }

            //// Test 5: The Saboteur (Manual Corruption Check)
            //{
            //    LOG_I("[Test 5] Validation Check");
            //    spd::GapBuffer gb;
            //    gb.Insert('A');
            //    gb.Insert('B');

            //    // FORCED CORRUPTION: Reach into the private data and break a poison byte
            //    // Note: You might need to make unit_test a friend class or use a dirty cast
            //     ((char*)gb.GetBuff())[gb.GetGapStart()] = 'X'; 

            //     gb.Validate(); // THIS SHOULD TRIGGER YOUR ASSERT!
            //}

            LOG_I("--- ALL GAP BUFFER TESTS PASSED ---\n");
        }
    }
}