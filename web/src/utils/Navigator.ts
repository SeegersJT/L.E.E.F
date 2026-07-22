import type { NavigateFunction, To, NavigateOptions } from 'react-router-dom'

let navigator: NavigateFunction | null = null

export const setNavigate = (nav: NavigateFunction): void => {
	navigator = nav
}

export function navigate(to: To, options?: NavigateOptions): void
export function navigate(delta: number): void
export function navigate(toOrDelta: To | number, options?: NavigateOptions): void {
	if (!navigator) {
		console.warn('Navigator: navigate() called before NavigateInjector has mounted.')
		return
	}
	if (typeof toOrDelta === 'number') {
		navigator(toOrDelta)
	} else {
		navigator(toOrDelta, options)
	}
}
