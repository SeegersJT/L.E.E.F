import type { NavigateFunction } from 'react-router-dom'

let _navigate: NavigateFunction | null = null

export const setNavigate = (fn: NavigateFunction) => {
	_navigate = fn
}

export const navigate = (path: string) => {
	_navigate?.(path)
}
