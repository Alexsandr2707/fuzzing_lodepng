" Makefile settings
autocmd FileType make setlocal noexpandtab shiftwidth=8 softtabstop=0

" General settings
set nocompatible    " Отключаем режим совместимости с Vi
set encoding=utf-8  " Устанавливаем кодировку UTF-8
set fileencoding=utf-8

" UI improvements
set number          " Нумерация строк
set relativenumber  " Относительная нумерация строк
set cursorline      " Подсветка текущей строки
set showcmd         " Показывать введенные команды
set ruler           " Показывать координаты курсора
set wildmenu        " Улучшенный автодополняющийся меню
set background=dark " Темная тема (если применимо)

" Indentation and formatting
set autoindent      " Автоматический отступ
set smartindent     " Умное выравнивание
set tabstop=4       " Размер табуляции - 4 пробела
set shiftwidth=4    " Размер отступа - 4 пробела
set expandtab       " Преобразование табов в пробелы
set smarttab        " Умная табуляция
set backspace=indent,eol,start " Улучшенное удаление символов

" Searching
set ignorecase      " Поиск без учета регистра
set smartcase       " Если есть заглавные буквы - учитывать регистр
set incsearch       " Интерактивный поиск
set hlsearch        " Подсветка результатов поиска

" Code navigation
set scrolloff=5     " Оставлять 5 строк сверху и снизу при прокрутке
set sidescrolloff=5 " Оставлять 5 символов по бокам при горизонтальной прокрутке
set wrap            " Перенос строк
set linebreak       " Перенос строк по словам
set showmatch       " Подсвечивать парные скобки
