/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chacha20.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 06:25:00 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 02:08:23 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <chacha20.h>

#define ROTL(a, b)						\
({										\
	((a) << (b)) | ((a) >> (32 - (b)));	\
})

#define QR(a, b, c, d)					\
do {									\
	a += b; d ^= a; ROTL(d, 16);		\
	c += d; b ^= c; ROTL(b, 12);		\
	a += b; d ^= a; ROTL(d,  8);		\
	c += d; b ^= c; ROTL(b,  7);		\
} while (0)								\

static void chacha_block(uint32_t matrix[16], uint32_t cypher[16])
{
	for (int i = 0; i < 16; i++)
		cypher[i] = matrix[i];

	for (int i = 0; i < 10; i++)
	{
		QR(cypher[ 0], cypher[ 4], cypher[ 8], cypher[12]);
		QR(cypher[ 1], cypher[ 5], cypher[ 9], cypher[13]);
		QR(cypher[ 2], cypher[ 6], cypher[10], cypher[14]);
		QR(cypher[ 3], cypher[ 7], cypher[11], cypher[15]);

		QR(cypher[ 0], cypher[ 5], cypher[10], cypher[ 3]);
		QR(cypher[ 1], cypher[ 6], cypher[11], cypher[ 4]);
		QR(cypher[ 2], cypher[ 7], cypher[ 8], cypher[ 9]);
		QR(cypher[ 3], cypher[ 4], cypher[ 9], cypher[14]);
	}

	for (int i = 0; i < 16; i++)
		cypher[i] += matrix[i];
}

int chacha20_keygen(t_key key)
{
	int urandfd;
	int	rdret;

	urandfd = open("/dev/urandom", O_RDONLY);
	if (urandfd < 0)
	{
		printf("Error: Failed to generate a key\n");
		return (-1);
	}
	rdret = read(urandfd, (char *)key, 32);
	close(urandfd);
	if (rdret < 32)
	{
		printf("Error: Failed to generate a key\n");
		return (-1);
	}
	return (0);
}

void chacha20_keyprint(t_key key)
{
	printf("chacha20 key:\n%x%x%x%x%x%x%x%x\n", key[0], key[1], key[2], key[3], key[4], key[5], key[6], key[7]);
}

void chacha20_run(uint8_t *data, size_t offset, size_t size, t_key key)
{
	uint32_t    cypher[16];
	uint32_t    matrix[16] =
	{
		0x61707865, 0x3320646e, 0x79622d32, 0x6b206574,
		key[0], key[1], key[2], key[3],
		key[4], key[5], key[6], key[7],
		0x0000, 0x0000, 0x0042, 0x0042
	};

	for (size_t i = 0; i < size; i++)
	{
		if (i % 64 == 0)
		{
		    *(uint64_t *)(matrix + 12) += 1;
		    chacha_block(matrix, cypher);
		}
		data[offset + i] ^= ((unsigned char *)cypher)[i % 64];
	}
}
