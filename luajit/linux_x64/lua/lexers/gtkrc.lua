-- Copyright 2006-2018 Mitchell mitchell.att.foicica.com. See License.txt.
-- Gtkrc LPeg lexer.

local lexer = require('lexer')
local token, word_match = lexer.token, lexer.word_match
local P, R, S = lpeg.P, lpeg.R, lpeg.S

local lex = lexer.new('gtkrc')

-- Whitespace.
lex:add_rule('whitespace', token(lexer.WHITESPACE, lexer.space^1))

-- Keywords.
lex:add_rule('keyword', token(lexer.KEYWORD, word_match[[
  binding class include module_path pixmap_path im_module_file style widget
  widget_class
]]))

-- Variables.
lex:add_rule('variable', token(lexer.VARIABLE, word_match[[
  bg fg base text xthickness ythickness bg_pixmap font fontset font_name stock
  color engine
]]))

-- States.
lex:add_rule('state', token('state', word_match[[
  ACTIVE SELECTED NORMAL PRELIGHT INSENSITIVE TRUE FALSE
]]))
lex:add_style('state', lexer.STYLE_CONSTANT)

-- Functions.
lex:add_rule('function', token(lexer.FUNCTION, word_match[[
  mix shade lighter darker
]]))

-- Identifiers.
lex:add_rule('identifier', token(lexer.IDENTIFIER, lexer.alpha *
                                                   (lexer.alnum + S('_-'))^0))

-- Strings.
lex:add_rule('string', token(lexer.STRING, lexer.delimited_range("'", true) +
                                           lexer.delimited_range('"', true)))

-- Comments.
lex:add_rule('comment', token(lexer.COMMENT, '#' * lexer.nonnewline^0))

-- Numbers.
lex:add_rule('number', token(lexer.NUMBER, lexer.digit^1 *
                                           ('.' * lexer.digit^1)^-1))

-- Operators.
lex:add_rule('operator', token(lexer.OPERATOR, S(':=,*()[]{}')))

-- Fold points.
lex:add_fold_point(lexer.OPERATOR, '{', '}')
lex:add_fold_point(lexer.COMMENT, '#', lexer.fold_line_comments('#'))

return lex
