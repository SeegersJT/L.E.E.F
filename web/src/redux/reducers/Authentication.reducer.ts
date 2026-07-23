import { AUTH_ACTIONS } from '../actions/Authentication.action'
import type { AuthState } from '../types/Authentication.type'

const initialState: AuthState = {
	user: null,
	loading: false,
	initialized: false,
}

type Action = { type: string; payload?: unknown }

export const AuthReducer = (state = initialState, action: Action): AuthState => {
	switch (action.type) {
		case AUTH_ACTIONS.REQUEST_FIREBASE_EMAIL_LOGIN_LOADING:
		case AUTH_ACTIONS.REQUEST_FIREBASE_REGISTER_LOADING:
		case AUTH_ACTIONS.REQUEST_FIREBASE_GOOGLE_LOGIN_LOADING:
		case AUTH_ACTIONS.REQUEST_PASSWORD_RESET_LOADING:
		case AUTH_ACTIONS.REQUEST_LOGOUT_LOADING:
			return {
				...state,
				loading: action.payload as boolean,
			}

		case AUTH_ACTIONS.SET_AUTH_USER:
			return {
				...state,
				user: action.payload as AuthState['user'],
				initialized: true,
			}

		case AUTH_ACTIONS.RESET_AUTH:
			return {
				...initialState,
			}

		default:
			return state
	}
}
