/**
 * @file ipc.h
 * @brief Inter Process Communication helpers
 */

#pragma once

#include <3ds/types.h>

typedef enum
{
	IPC_BUFFER_R  = BIT(1),
	IPC_BUFFER_W  = BIT(2),
	IPC_BUFFER_RW = IPC_BUFFER_R | IPC_BUFFER_W
} IPC_BufferRights;


/**
 * @brief Command header to be used for IPC
 * @param normal_params    Number of normal parameters. Up to 63.
 * @param translate_params Number of translate parameters. Up to 63.
 */
static inline u32 IPC_MakeHeader(u16 command_id, unsigned normal_params, unsigned translate_params)
{
	return ((u32) command_id << 16) | (((u32) normal_params & 0x3F) << 6) | (((u32) translate_params & 0x3F) << 0);
}


/**
 * @brief Creates the header to share handles
 * @param number The number of handles following this header. Max 64.
 *
 * The #number next values are handles that will be shared between the two processes.
 *
 * @note Zero values will have no effect.
 */
static inline u32 IPC_Desc_SharedHandles(unsigned number)
{
	return ((u32)(number - 1) << 26);
}

/**
 * @brief Creates the header to transfer handle ownership
 * @param number The number of handles following this header. Max 64.
 *
 * The #number next values are handles that will be duplicated and closed by the other process.
 *
 * @note Zero values will have no effect.
 */
static inline u32 IPC_Desc_MoveHandles(unsigned number)
{
	return ((u32)(number - 1) << 26) | 0x10;
}

/**
 * @brief Asks the kernel to fill the handle with the current process handle.
 *
 * The next value is a placeholder that will be replaced by the current process handle by the kernel.
 */
static inline u32 IPC_Desc_CurProcessHandle(void)
{
	return 0x20;
}

/**
 * @brief Creates a header describing a static buffer.
 * @param size      Size of the buffer. Max ?0x03FFFF?.
 * @param buffer_id The Id of the buffer. Max 0xF.
 *
 * The next value is a pointer to the buffer. It will be copied to TLS offset 0x180 + static_buffer_id*8.
 */
static inline u32 IPC_Desc_StaticBuffer(size_t size, unsigned buffer_id)
{
	return (size << 14) | ((buffer_id & 0xF) << 10) | 0x2;
}

/**
 * @brief Creates a header describing a buffer to be sent over PXI.
 * @param size         Size of the buffer. Max 0x00FFFFFF.
 * @param buffer_id    The Id of the buffer. Max 0xF.
 * @param is_read_only true if the buffer is read-only. If false, the buffer is considered to have read-write access.
 *
 * The next value is a phys-address of a table located in the BASE memregion.
 */
static inline u32 IPC_Desc_PXIBuffer(size_t size, unsigned buffer_id,bool is_read_only)
{
	u8 type = 0x4;
	if(is_read_only)type = 0x6;
	return (size << 8) | ((buffer_id & 0xF) << 4) | type;
}

/**
 * @brief Creates a header describing a buffer from the main memory.
 * @param size   Size of the buffer. Max 0x0FFFFFFF.
 * @param rights The rights of the buffer for the destination process
 *
 * The next value is a pointer to the buffer.
 */
static inline u32 IPC_Desc_Buffer(size_t size, IPC_BufferRights rights)
{
	return (size << 4) | 0x8 | rights;
}
