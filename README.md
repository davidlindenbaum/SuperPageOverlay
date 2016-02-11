# SuperPageOverlay
Page overlays for flexible superpages

#### Overlay architecture
* Superpages are created by Linux transparent hugepage and/or our own detection/creation of superpages
* The 2mb TLB has an OBV attached to each entry
* On write to a COW superpage:
  * Request free page(s) from the OS
  * Copy the segment of the superpage being written
  * Set the corresponding bit in the OBV in the TLB and page table
  * Add the new mapping to the OMT
* On a TLB hit in the 2mb TLB:
  * Hardware checks the OBV
  * If the corresponding bit is set, it goes to the 4kb TLB instead - it expects a regular mapping to the real physical address of the 4kb overlay page
  * On a miss in the 4kb TLB, it does the page table walk in the OMT instead of the regular page table hierarchy
* The OMT is just a separate 4-level page table that works as normal
  * Easiest if it's per-process like normal
  * To match the regular page overlays it would be global and more complex
* In the regular page tables, any 2mb page mappings have an OBV attached, which is copied into the TLB entry when there's a miss in the 2mb TLB

Slightly different, alternate approach:
Rather than being a separate page table hierarchy, the OMT could just be part of the the regular page tables.
* Each entry in the 3rd level page table (or 2nd) has a mapping to the next level page table, a mapping to a data page, and an OBV
* The OBV determines which addresses (looking at the bits after the page table index) use the page table mapping, and which use that data mapping
* All 0 OBV means it's a regular superpage
* All 1 OBV means a regular mapping to the next page table

#### Testing infrastructure
Similar to BadgerTrap, except we model a virtual TLB and cause all accesses to addresses not in our virtual TLB to trap. We set every page table entry not in our virtual TLB as "fake invalid" - present bit not set, but a bit set indicating it's supposed to be a valid mapping - and flush it from the real TLB. Then, any access that would miss in our virtual TLB page faults. We update the virtual TLB, set the page to "valid", and continue. Based on "Trap-Driven Simulation" from http://www.solarisinternals.com/si/reading/madu_thesis.pdf.
