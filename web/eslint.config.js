import js from '@eslint/js'
import ts from '@typescript-eslint/eslint-plugin'
import tsParser from '@typescript-eslint/parser'
import reactHooks from 'eslint-plugin-react-hooks'
import prettier from 'eslint-plugin-prettier'
import prettierConfig from 'eslint-config-prettier'
import globals from 'globals'

export default [
	{ ignores: ['dist/**', 'node_modules/**'] },
	js.configs.recommended,
	prettierConfig,
	{
		files: ['**/*.{ts,tsx,js,jsx,json}'],
		languageOptions: {
			parser: tsParser,
			parserOptions: {
				ecmaVersion: 'latest',
				sourceType: 'module',
				ecmaFeatures: { jsx: true },
			},
			globals: {
				...globals.browser,
				...globals.node,
			},
		},
		plugins: {
			'@typescript-eslint': ts,
			'react-hooks': reactHooks,
			prettier,
		},
		rules: {
			'prettier/prettier': ['error', { endOfLine: 'crlf' }],

			'@typescript-eslint/no-unused-vars': 'warn',
			'@typescript-eslint/no-explicit-any': 'warn',

			'react-hooks/rules-of-hooks': 'error',
			'react-hooks/exhaustive-deps': 'warn',

			'no-console': 'warn',
			'no-unused-vars': 'off',
		},
		settings: {
			react: { version: 'detect' },
		},
	},
]
