/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bccyv <bccyv@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 15:40:55 by bccyv             #+#    #+#             */
/*   Updated: 2021/07/24 17:52:42 by bccyv            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <elf.h>
#include <stdio.h>
#include <woody.h>
#include <string.h>
#include <chacha20.h>

uint8_t stub[] =
{
	0xeb, 0xfe
};

static void prepare_stub(uint64_t oep, uint64_t tsz, t_key key)
{
	size_t	key_off = 0;
	size_t	tsz_off = 0;
	size_t	jmp_off = 0;

	memcpy(stub + key_off, key, 16);
	memcpy(stub + jmp_off, &oep, sizeof oep);
	memcpy(stub + tsz_off, &tsz, sizeof tsz);
}

int main(int ac, char **av)
{
	t_elf		*elf;
	t_key		key;
	char        out[] = "woody";
	uint64_t	tsz = 0; // TODO

	if (ac != 2)
	{
		printf("Usage: %s <t_elf executable>\n", av[0]);
		return (1);
	}
	if ((elf = elf64_read(av[1])) == NULL)
		return (1);
	if (elf64_is_already_packed(elf) || chacha20_keygen(key) < 0 || elf64_encrypt_section(elf, ".text", key) < 0)
	{
		elf64_free(elf);
		return (1);
	}
	prepare_stub(elf->header.e_entry, tsz, key);
	if (elf64_inject(elf, stub, sizeof(stub)) < 0 || elf64_write(elf, out) < 0)
	{
		elf64_free(elf);
		return (1);
	}
	elf64_free(elf);
	return (0);
}
