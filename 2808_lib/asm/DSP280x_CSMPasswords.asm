Change  .set    0
      .sect "csmpasswds"

    .if Change == 0
      .int  0xFFFF      ;PWL0 (LSW of 128-bit password)
      .int  0xFFFF      ;PWL1
      .int  0xFFFF      ;PWL2
      .int  0xFFFF      ;PWL3
      .int  0xFFFF      ;PWL4
      .int  0xFFFF      ;PWL5
      .int  0xFFFF      ;PWL6
      .int  0xFFFF      ;PWL7 (MSW of 128-bit password)
    .endif
    .if Change == 1
      .int  0x46CA      ;PWL0 (LSW of 128-bit password)
      .int  0x9CD6      ;PWL1
      .int  0xC023      ;PWL2
      .int  0xE840      ;PWL3
      .int  0x54AB      ;PWL4
      .int  0x9334      ;PWL5
      .int  0x4FF2      ;PWL6
      .int  0x285A      ;PWL7 (MSW of 128-bit password)
    .endif
     .sect "csm_rsvd"
        .loop (3F7FF5h - 3F7F80h + 1)
              .int 0x0000
        .endloop

