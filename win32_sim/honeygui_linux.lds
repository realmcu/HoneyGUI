SECTIONS
{
  .honeygui_init : {
      __honeygui_init_start = .;
      KEEP(*(SORT(.honeygui$init*)))
      __honeygui_init_end = .;
  }

  .shellCommand : {
      _shell_command_start = .;
      KEEP(*(.shellCommand))
      _shell_command_end = .;
  }
}
INSERT AFTER .text;
