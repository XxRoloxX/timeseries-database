require("rolo")

require("config.lazy")

vim.cmd('set number')

vim.wo.relativenumber = true
vim.opt["tabstop"] = 2
vim.opt["shiftwidth"] = 2


vim.api.nvim_create_autocmd('LspAttach', {
	callback = function(args)
		local opts = { buffer = args.buf }


		-- vim.keymap.set({ "x", "n" }, "<leader>c", "<cmd>lua vim.lsp.buf.document_highlight()<cr>", opts)

		vim.keymap.set('n', '<C-Space>', '<C-x><C-o>', opts)
		vim.keymap.set('n', 'gd', '<cmd>lua vim.lsp.buf.definition()<cr>', opts)
		vim.keymap.set({ 'n', 'x' }, 'gq', '<cmd>lua vim.lsp.buf.format({async = true})<cr>', opts)
		vim.keymap.set('n', 'grt', '<cmd>lua vim.lsp.buf.type_definition()<cr>', opts)
		vim.keymap.set('n', 'grd', '<cmd>lua vim.lsp.buf.declaration()<cr>', opts)
		vim.keymap.set('n', 'gi', '<cmd>lua vim.lsp.buf.implementation()<cr>', opts)
		vim.keymap.set('n', '<leader>ga', '<cmd>lua vim.lsp.buf.code_action()<cr>', opts)

		local client = vim.lsp.get_client_by_id(args.data.client_id)

		if client:supports_method('textDocument/formatting') then
			vim.api.nvim_create_autocmd('BufWritePre', {
				buffer = args.buf,
				callback = function()
					vim.lsp.buf.format({ bufnr = args.buf, id = client.id })
				end,
			})
		end
	end,
})

local capabilities = require('cmp_nvim_lsp').default_capabilities()

require("lspconfig").ts_ls.setup(
	{
		capabilities = capabilities,
	}
)
require("lspconfig").lua_ls.setup(
	{
		capabilities = capabilities,
	}
)
require("lspconfig").clangd.setup(
	{
		capabilities = capabilities,
	}
)


vim.keymap.set("n", "<leader>st", function()
	vim.cmd.vnew()
	vim.cmd.term()
	vim.cmd.wincmd("J")
	vim.api.nvim_win_set_height(0, 15)
	job_id = vim.bo.channel
end)

vim.keymap.set("n", "<leader>s", ":w<CR>")


-- vim.keymap.set("n", "<leader>example", function()
-- 	vim.fn.chansend(job_id, { "echo 'hi'\n" })
-- end)
