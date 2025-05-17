-- disable netrw at the very start of your init.lua
vim.g.loaded_netrw = 1
vim.g.loaded_netrwPlugin = 1

-- optionally enable 24-bit colour
vim.opt.termguicolors = true

-- custom mappings
vim.keymap.set('n', '<leader>e', function() vim.cmd('NvimTreeOpen') end, {})

local function my_on_attach(bufnr)
	local api = require "nvim-tree.api"

	local function opts(desc)
		return { desc = "nvim-tree: " .. desc, buffer = bufnr, noremap = true, silent = true, nowait = true }
	end

	-- default mappings
	api.config.mappings.default_on_attach(bufnr)



	-- custom mappings
	vim.keymap.set('n', '<C-t>', api.tree.change_root_to_parent, opts('Up'))
	vim.keymap.set('n', '<C-g>f', function() vim.cmd('NvimTreeOpen') end, opts('Help'))
	vim.keymap.set('n', '<leader>e', function() vim.cmd('NvimTreeClose') end, opts('Help'))
end

-- pass to setup along with your other options
require("nvim-tree").setup {
	---
	on_attach = my_on_attach,
	---
}
