def generate_r_type_instruction():
    opcode = int(input("Enter opcode: "))
    rs = int(input("Enter rs: "))
    rt = int(input("Enter rt: "))
    rd = int(input("Enter rd: "))
    comment = input("Enter comment: ")

    opcode_str = format(opcode, '06b')
    rs_str = format(rs, '05b')
    rt_str = format(rt, '05b')
    rd_str = format(rd, '05b')
    unused_str = '0' * 11
    instruction = opcode_str + rs_str + rt_str + rd_str + unused_str
    hex_instruction = format(int(instruction, 2), '08X')
    print(f"{hex_instruction} opcode: {opcode} rs: {rs} rt: {rt} rd: {rd} // {comment}")

def generate_i_type_instruction():
    opcode = int(input("Enter opcode: "))
    rs = int(input("Enter rs: "))
    rt = int(input("Enter rt: "))
    imm = int(input("Enter immediate value: "))
    comment = input("Enter comment: ")

    opcode_str = format(opcode, '06b')
    rs_str = format(rs, '05b')
    rt_str = format(rt, '05b')
    imm_str = format(imm & 0xFFFF, '016b')  # Mask to 16 bits
    instruction = opcode_str + rs_str + rt_str + imm_str
    hex_instruction = format(int(instruction, 2), '08X')
    print(f"{hex_instruction} opcode: {opcode} rs: {rs} rt: {rt} imm: {imm} // {comment}")

# Generate instructions
while True:
    instruction_type = input("Enter instruction type (R/I): ")
    if instruction_type.upper() == 'R':
        generate_r_type_instruction()
    elif instruction_type.upper() == 'I':
        generate_i_type_instruction()
    else:
        print("Invalid instruction type. Please enter 'R' for R-type or 'I' for I-type.")
