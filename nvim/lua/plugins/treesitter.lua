return {
	'nvim-treesitter/nvim-treesitter',
	name = "nvim-treesitter",
	config = function()
		vim.cmd(":TSUpdate")
	end
}


