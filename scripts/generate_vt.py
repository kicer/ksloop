# I am *so. done.* writing vector tables by hand. Help me out, Python?
import sys

# Don't run the script if the wrong # of arguments are passed in.
if not len( sys.argv ) == 3:
  print( 'Usage: python generate_vt.py <MCU> <Core>\n' +
         'Example: python generate_vt.py HC32F003x4 cortex-m0' )
  sys.exit( 1 )

# Array to hold relevant lines in the device header file.
irq_lines = []
# State-tracking variable for the header file parsing.
irq_appending = False
# Filename to write the vector table to.
vt_fn = 'startup/'+sys.argv[1].lower() + '_vt.S'
# Device header filename to read the interrupt information from.
dev_hdr_fp = 'bsp/inc/' + sys.argv[1].lower() + '.h'

# Open the device header file, and read its interrupt definition
# enum into the 'irq_lines' array.
with open( dev_hdr_fp, 'r' ) as dev_hdr:
  for line in dev_hdr:
    if irq_appending:
      irq_lines.append( line )
      if 'IRQn_Type' in line:
        # End of the interrupt enum; no need to keep reading.
        break
    elif 'Interrupt Number Definition' in line:
      # Near the start of the interrupt enum; start reading.
      irq_appending = True

# Dictionary to hold [ interrupt_index : interrupt_name ] pairs.
irq_dict = { }

# Run through the interrupt definitions enum, and parse each relevant
# line into a (key, value) pair in the 'irq_dict' variable.
for line in irq_lines:
  if '=' in line:
    while '  ' in line:
      line = line.replace( '  ', ' ' )
    line = line.replace( ',', '' )
    blocks = line.split( ' ' )
    irq_dict[ int( blocks[ 3 ] ) ] = blocks[ 1 ]

# Create a sorted list from the dictionary, so that interrupts
# will be in the right order in the vector table.
irq_list = sorted( irq_dict.items() )

# Open the target file and write a vector table.
with open( vt_fn, 'w+' ) as vt_as:
  # Header comment.
  vt_as.write( '/* Autogenerated vector table for ' +
               sys.argv[ 1 ] + ' */\n\n' )
  # Assembler directives.
  vt_as.write( '.syntax unified\n' )
  vt_as.write( '.cpu ' + sys.argv[ 2 ] + '\n' )
  vt_as.write( '.thumb\n\n' )
  # Labels to export to the wider program context.
  vt_as.write( '.global vtable\n' )
  vt_as.write( '.global default_interrupt_handler\n\n' )

  # The vector table definition. Goes into a special 'vector_table'
  # section so that the linker script can ensure it is placed at the
  # very beginning of the chip's memory space.
  vt_as.write( '.type vtable, %object\n' )
  vt_as.write( '.section .vector_table,"a",%progbits\n' )
  vt_as.write( 'vtable:\n' )
  # The first two entries are the 'end of stack' address and the
  # reset handler - neither are present in the device header file.
  vt_as.write( '  .word _estack\n' )
  vt_as.write( '  .word reset_handler\n' )
  # For each number in the range of interrupt indices, check if
  # an interrupt exists at that index. If so, make an entry for it.
  # If not, write a '0' to keep the correct ordering.
  for i in range( irq_list[ 0 ][ 0 ], ( irq_list[ -1 ][ 0 ] + 1 ) ):
    if i in irq_dict:
      vt_as.write( '  .word ' + irq_dict[ i ] + '_handler\n' )
    else:
      vt_as.write( '  .word 0\n' )
  vt_as.write( '\n' )

  # Create weak references for each interrupt handler, so that they
  # point to a dummy handler if the application writer does not
  # define an interrupt handler in their program.
  for i in range( irq_list[ 0 ][ 0 ], ( irq_list[ -1 ][ 0 ] + 1 ) ):
    if i in irq_dict:
      vt_as.write( '  .weak ' + irq_dict[ i ] + '_handler\n' )
      vt_as.write( '  .thumb_set ' + irq_dict[ i ] + '_handler,default_interrupt_handler\n' )

  # Close the definition of the vector table.
  vt_as.write( '.size vtable, .-vtable\n\n' )
  vt_as.write( '.section .text.default_interrupt_handler,"ax",%progbits\n' )

  # Define a default 'dummy' interrupt handler. This is about the same
  # as `while(1) {};`, it's an infinite loop. So if an interrupt is
  # triggered while a handler is not defined, the program will freeze.
  # That is better than accidentally corrupting memory, though.
  vt_as.write( 'default_interrupt_handler:\n' )
  vt_as.write( '  default_interrupt_loop:\n' )
  vt_as.write( '    B default_interrupt_loop\n' )
  vt_as.write( '.size default_interrupt_handler, .-default_interrupt_handler\n' )

# Done.
sys.exit( 0 )
