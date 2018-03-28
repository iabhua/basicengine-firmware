#include "basic.h"
#include "mcurses.h"
#include "BString.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>

#define TABSIZE 8
#define CHUNKSIZE 1024
#define STRMAX 64
#define BUFSIZE 256

#ifdef CTRL
#undef CTRL
#endif
#define CTRL(a) ((a) & 31)


static BString	file_name, block_name;
static BString	find_str, replace_str;

struct e_ctx_t {
  char	*text = 0;
  int	text_size = 0;
  int bos_pos, eos_pos, cur_pos, eof_pos, bow_line;
  int bow_line_prev, win_shift, cur_line, cur_y, cur_x;
  int is_changed, ins_mode, find_mode;
};
struct e_ctx_t *ctx;
#define text ctx->text
#define text_size ctx->text_size
#define bos_pos ctx->bos_pos
#define eos_pos ctx->eos_pos
#define cur_pos ctx->cur_pos
#define eof_pos ctx->eof_pos
#define bow_line ctx->bow_line
#define bow_line_prev ctx->bow_line_prev
#define win_shift ctx->win_shift
#define cur_line ctx->cur_line
#define cur_y ctx->cur_y
#define cur_x ctx->cur_x
#define is_changed ctx->is_changed
#define ins_mode ctx->ins_mode
#define find_mode ctx->find_mode

#define	ALIGN(x,mod)	(((x) / (mod) + 1) * (mod))
#define nexttab(x)	ALIGN (x, TABSIZE)
#define align_chunk(x)	ALIGN (x, CHUNKSIZE)

static void beep()
{
  PRINT_P("beep!\r\n");
}

void	adduch (unsigned char ch)
{
	if ((ch >= 32 && ch < 128) || ch >= 160)	/* ^C or ~C */
		addch (ch);
	else {
		attrset(A_BLINK);//attron (A_BLINK);
		addch (ch > 128 && ch < 159 ? ch - 32 : ch + 64);
		attrset(A_NORMAL);//attroff (A_BLINK);
	}
}

static int	confirm (const char *s)
{
	int	ch;

	move (LINES - 1, 0);
	attrset(A_BOLD);//attron (A_BOLD);
	addstr_P (s);
	attrset(A_NORMAL);//attroff (A_BOLD);
	clrtoeol ();
	refresh ();
	ch = getch ();
	return ch == 'y' || ch == 'Y';
}

int	enter_string (const char *s, BString &buf)
{
	int	ch, flag = 1;

	for (;;) {
		move (LINES - 1, 0);
		attrset(A_BOLD);//attron(A_BOLD);
		addstr_P (s);
		attrset(A_NORMAL);//attroff (A_BOLD);
		for (unsigned int b_len = 0; b_len < buf.length(); b_len++)
			adduch (buf[b_len]);
		clrtoeol ();
		refresh ();
		ch = getch ();
		switch (ch) {
		case CTRL ('Y'):
			//*buf = 0;
			break;
		case CTRL ('Q'):
			ch = getch ();
			goto ins_char;
		case KEY_BACKSPACE:
		        buf = buf.substring(0, buf.length() - 1);
			break;
		case '\r': case '\n':
			return 1;
		case CTRL ('X'):
			return 0;
		default:
			if (!iscntrl (ch)) {
		ins_char:	if (flag)
					buf = F("");
				if (buf.length() < STRMAX - 1) {
				        buf += (char)ch;
				}
			} else
				beep ();
			break;
		}
		flag = 0;
	}
	/* NOTREACHED */
}

static int	error (const char *s, ...)
{
	va_list	args;
	char	buf[BUFSIZE];
	int	i = 0;

	va_start (args, s);
	i += snprintf (buf + i, BUFSIZE - i, "Error ");
	if (*s != '$')
		i += vsnprintf (buf + i, BUFSIZE - i, s, args);
	else {
		i += vsnprintf (buf + i, BUFSIZE - i, s + 1, args);
		i += snprintf (buf + i, BUFSIZE - i, ", %d", errno);
	}
	va_end (args);
	beep ();
	confirm (buf);

	return 0;	/* convinient */
}

int	bol (int pos)
{
	while (pos && text[pos - 1] != '\n')
		pos--;
	return pos;
}

int	prevline (int pos)
{
	pos = bol (pos);
	return pos ? bol (pos - 1) : 0;
}

int	eol (int pos)
{
	while (pos < eof_pos && text[pos] != '\n')
		pos++;
	return pos;
}

int	nextline (int pos)
{
	pos = eol (pos);
	return pos < eof_pos ? pos + 1 : pos;
}

int	win_x (int line, int xx)
{
	int	i, x = 0;

	for (i = line; i < eof_pos && i < line + xx; i++)
		if (text[i] == '\n')
			break;
		else if (text[i] == '\t')
			x = nexttab (x);
		else
			x++;
	return x;
}

int	pos_x (int line, int xx)
{
	int	i, x = 0;

	for (i = line; i < eof_pos && x < xx; i++)
		if (text[i] == '\n')
			break;
		else if (text[i] == '\t')
			x = nexttab (x);
		else
			x++;
	return i;
}

void	show (void)
{
	int	i, m, t, j;

	/*
	 * speed up scrolling
	 */
	if (bow_line > bow_line_prev) {
		m = bow_line_prev;
		for (i = 0; m != bow_line && i < LINES; i++)
			m = nextline (m);
		if (i < LINES)
			scrl (i);
	} else if (bow_line < bow_line_prev) {
		m = bow_line_prev;
		for (i = 0; m != bow_line && i < LINES; i++)
			m = prevline (m);
		if (i < LINES)
			scrl (-i);
	}
	bow_line_prev = bow_line;

//	erase ();
	if (!text)
		return;
	for (m = bow_line, i = 0; m < eof_pos && i < LINES; i++) {
		m = pos_x (m, win_shift);
		move (i, 0);
#define EOS_COLS (i < LINES - 1 ? COLS : COLS - 1)
		for (j = 0; m < eof_pos && j < EOS_COLS; m++) {
			if (m >= bos_pos && m < eos_pos)
				attrset(A_REVERSE);//attron(A_REVERSE);
			else
				attrset(A_NORMAL);//attroff (A_REVERSE);
			if (text[m] == '\n') {
			        clrtoeol();
				break;
                        }
			else if (text[m] == '\t')
				for (t = nexttab (j); j < t; j++)
					addch (' ');
			else {
				adduch (text[m]);
				j++;
			}
		}
		if (m >= bos_pos && m < eos_pos)
			while (j++ < EOS_COLS)
				addch (' ');
#undef EOS_COLS
		m = nextline (m);
	}
	while (i < LINES) {
	  move (i, 0);
	  clrtoeol();
	  ++i;
        }
	attrset(A_NORMAL);//attroff (A_REVERSE);
}

void	k_up (void)
{
	cur_line = prevline (cur_line);
	cur_pos = pos_x (cur_line, cur_x + win_shift);
}

void	k_down (void)
{
	if (eol (cur_pos) < eof_pos) {
		cur_line = nextline (cur_line);
		cur_pos = pos_x (cur_line, cur_x + win_shift);
	}
}

int	ins_mem (int size)
{
	char	*p;
	int	i;

	if (!text || eof_pos + size > text_size) {
		i = align_chunk (eof_pos + size);
		p = (char *)realloc (text, i);
		if (!p)
			return error ("- no memory");
		text = p;
		text_size = i;
	}
	/*
	 * read last sentence in BUGS section of memcpy(3) in FreeBSD,
	 * also bcopy(3) is not in ``ISO C''
	 */
	for (i = eof_pos - 1; i >= cur_pos; i--)
		text[i + size] = text[i];
	eof_pos += size;
	if (bos_pos >= cur_pos)
		bos_pos += size;
	if (eos_pos > cur_pos)
		eos_pos += size;
	is_changed = 1;
	return 1;
}

void	del_mem (int pos, int size)
{
	int	i;
	char	*p;

	for (i = pos + size; i < eof_pos; i++)	/* read comment to ins_mem() */
		text[i - size] = text[i];
	eof_pos -= size;
	is_changed = 1;
#define del_pos(p) (p > pos + size ? p -= size : p > pos ? p = pos : p)
	del_pos (bos_pos);
	del_pos (eos_pos);
	del_pos (cur_pos);
	del_pos (bow_line);
	del_pos (bow_line_prev);
#undef del_pos
	i = align_chunk (eof_pos);
	if (i < text_size) {
		p = (char *)realloc (text, i);
		if (!p) {
			error ("- realloc to decrease failed?");
			return;
		}
		text = p;
		text_size = i;
	}
}

void	ins_ch (char ch)
{
	if (!ins_mode && cur_pos < eof_pos) {
		if (ch == '\n') {
			cur_pos = nextline (cur_pos);
			return;
		} else if (text[cur_pos] != '\n') {
			is_changed = 1;
			goto a;
		}
	}
	if (ins_mem (1))
a:		text[cur_pos++] = ch;
}

void	k_copyblock (void)
{
	if (eos_pos <= bos_pos || (cur_pos > bos_pos && cur_pos < eos_pos))
		beep ();
	else if (ins_mem (eos_pos - bos_pos))
		strncpy (text + cur_pos, text + bos_pos, eos_pos - bos_pos);
}

void	k_moveblock (void)
{
	int	i;

	if (eos_pos <= bos_pos || (cur_pos > bos_pos && cur_pos < eos_pos)) {
		beep ();
		return;
	}
	k_copyblock ();
	i = eos_pos - bos_pos;
	del_mem (bos_pos, i);
	bos_pos = cur_pos;
	eos_pos = cur_pos + i;
}

void	k_deleteblock (void)
{
	if (eos_pos <= bos_pos)
		beep ();
	else
		del_mem (bos_pos, eos_pos - bos_pos);
}

int	find_again (int flag)
{
	int	f_len, i;

	f_len = find_str.length();
	if (!f_len)
		return 0;
	for (i = cur_pos + flag; i <= eof_pos - f_len; i++)
		if (!strncmp (text + i, find_str.c_str(), f_len))
			break;
	if (i > eof_pos - f_len)
		beep ();
	else
		cur_pos = i;
	return i <= eof_pos - f_len;
}

int	k_find (void)
{
	if (!enter_string (PSTR("Search for: "), find_str) || !find_str.length())
		return 0;
	find_mode = 1;
	return find_again (0);
}

void	replace_again (void)
{
	int	i;

	if (!find_again (0))
		return;
	del_mem (cur_pos, find_str.length());
	if (!replace_str.length())
		return;
	i = replace_str.length();
	if (ins_mem (i)) {
		strncpy (text + cur_pos, replace_str.c_str(), i);
		cur_pos += i;
	}
}

void	k_replace (void)
{
	if (!k_find ())
		return;
	if (!enter_string (PSTR("Replace to: "), replace_str))
		return;
	find_mode = 0;
	replace_again ();
}

void	k_again (void)
{
	if (find_mode)
		find_again (1);
	else
		replace_again ();
}

static int	load (const char *name)
{
#if 0
	FILE	*f;
	int	i;

	f = fopen (name, "r");
	if (!f)
		return error ("$load file \"%s\"", name);
	if (fseek (f, 0, SEEK_END))
		return error ("$seek");
	i = ftell (f);
	if (ins_mem (i)) {
		if (fseek (f, 0, SEEK_SET))
			return error ("$seek");
		if (fread (text + cur_pos, 1, i, f) < i)
			return error ("$read");
	} else
		i = 0;
	fclose (f);
	return i;
#else
return 0;
#endif
}

static int	save (const char *name, int pos, int size)
{
#if 0
	FILE	*f;

	f = fopen (name, "w");
	if (!f)
		return error ("$save file \"%s\"", name);
	if (fwrite (text + pos, 1, size, f) < size)
		return error ("$write");
	if (fclose (f))
		return error ("$close");
	return 1;
#else
return 0;
#endif
}

void	k_save (void)
{
	if (!enter_string (PSTR("Enter file name to save: "), file_name))
		return;
	if (save (file_name.c_str(), 0, eof_pos))
		is_changed = 0;
}

void	k_getblock (void)
{
	if (!enter_string (PSTR("Enter file name to load block: "), block_name))
		return;
	eos_pos = load (block_name.c_str()) + cur_pos;
	bos_pos = cur_pos;
}

void	k_putblock (void)
{
	if (bos_pos >= eos_pos)
		return;
	if (!enter_string (PSTR("Enter file name to save block: "), block_name))
		return;
	save (block_name.c_str(), bos_pos, eos_pos - bos_pos);
}

void	goto_line (int l)
{
	for (cur_pos = 0; --l > 0 && cur_pos < eof_pos;)
		cur_pos = nextline (cur_pos);
}

void	k_goto (void)
{
	BString buf;

	if (!enter_string (PSTR("Goto line: "), buf))
		return;
	if (buf.length())
		goto_line (atoi (buf.c_str()));
	else
		cur_pos = bos_pos;
}

/* ARGSUSED0 */
void	done (int sig)
{
	endwin ();
#ifndef ARDUINO
	exit (0);
#endif
}

static void	_init (void)
{
#ifndef ARDUINO
	signal (SIGINT, done);
#endif
	initscr ();
#if 0
	keypad (stdscr, TRUE);
	scrollok (stdscr, TRUE);
	idlok (stdscr, TRUE);
	nonl ();
	raw ();
	noecho ();
#endif
}

void	norm_cur (void)
{
	int	i;

	cur_line = bol (cur_pos);
	while (cur_line < bow_line)
		bow_line = prevline (bow_line);
	cur_y = 0;
	for (i = bow_line; i < cur_line; i = nextline (i))
		cur_y++;
	for (; cur_y >= LINES; cur_y--)
		bow_line = nextline (bow_line);
	cur_x = win_x (cur_line, cur_pos - cur_line) - win_shift;
	while (cur_x < 0) {
		cur_x += TABSIZE;
		win_shift -= TABSIZE;
	}
	while (cur_x >= COLS) {
		cur_x -= TABSIZE;
		win_shift += TABSIZE;
	}
}

int	e_main (int argc, char **argv)
{
	int	i, ch;

	ctx = (struct e_ctx_t *)calloc(1, sizeof(struct e_ctx_t));
	if (!ctx)
	  return -1;

	file_name = "";
	block_name = "";
	find_str = "";
	replace_str = "";

	_init ();
	if (argc >= 2) {
	        file_name = argv[1];
		load (file_name.c_str());
		is_changed = 0;
	}
	erase();
	for (;;) {
	        curs_set(0);
		show ();
		move (cur_y, cur_x);
		curs_set(1);
		refresh ();
		ch = getch ();
		switch (ch) {
		case KEY_UP:
			k_up ();
			break;
		case KEY_DOWN:
			k_down ();
			break;
		case KEY_LEFT:
			if (cur_pos)
				cur_pos--;
			break;
		case KEY_RIGHT:
			if (cur_pos < eof_pos)
				cur_pos++;
			break;
		case KEY_PPAGE:// case CTRL ('J'):
			for (i = 1; i < LINES; i++)
				k_up ();
			break;
		case KEY_NPAGE:// case CTRL ('K'):
			for (i = 1; i < LINES; i++)
				k_down ();
			break;
		case KEY_DC:	/* del */
			if (cur_pos < eof_pos)
				del_mem (cur_pos, 1);
			break;
		case KEY_BACKSPACE:
			if (cur_pos)
				del_mem (--cur_pos, 1);
			break;
		case KEY_HOME:
			cur_pos = cur_line;
			break;
		case KEY_END:
			cur_pos = eol (cur_pos);
			break;
		case CTRL ('X'):
			if (!is_changed || confirm (PSTR("Discard changes and exit? (y/N):"))) {
				done (0);
				goto out;
                        }
			break;
		case CTRL ('T'):	/* go Top */
			bow_line = cur_pos = 0;
			break;
		case CTRL ('O'):	/* go bOttom */
			cur_pos = eof_pos;
			break;
		case CTRL ('Y'):	/* del line */
			del_mem (cur_line, nextline (cur_line) - cur_line);
			break;
		case CTRL ('B'):	/* mark Begin of block */
			bos_pos = cur_pos;
			break;
		case CTRL ('E'):	/* mark End of block */
			eos_pos = cur_pos;
			break;
		case CTRL ('Q'):	/* Quote char */
			ins_ch (getch ());
			break;
		case CTRL ('C'):
			k_copyblock ();
			break;
		case CTRL ('D'):
			k_deleteblock ();
			break;
		case CTRL ('V'):
			k_moveblock ();
			break;
		case CTRL ('S'):
			k_save ();
			break;
		case CTRL ('F'):
			k_find ();
			break;
		case CTRL ('R'):
			k_replace ();
			break;
		case CTRL ('N'):
			k_again ();
			break;
		case CTRL ('P'):
			k_putblock ();
			break;
		case CTRL ('G'):
			k_getblock ();
			break;
		case KEY_IC:
			ins_mode ^= 1;
			break;
		case CTRL ('A'):
			k_goto ();
			break;
		case '\r':
			ch = '\n';
			/* FALLTHRU */
		default:
			if (!iscntrl (ch) || ch == '\t' || ch == '\n')
				ins_ch (ch);
			else
				beep ();
			break;
		}
		//printf("C%d(%c)", ch, ch);
		norm_cur ();
	}
out:
	if (text)
	  free(text);

	free(ctx);

	return 0;
}