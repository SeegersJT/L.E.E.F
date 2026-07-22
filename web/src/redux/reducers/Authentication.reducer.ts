import { AUTH_ACTIONS } from '../actions/Authentication.action'
import type { AuthState } from '../types/Authentication.type'

const initialState: AuthState = {
	user: null,
	loading: false,
}

type Action = { type: string; payload?: unknown }

export const AuthReducer = (state = initialState, action: Action): AuthState => {
	switch (action.type) {
		case AUTH_ACTIONS.REQUEST_FIREBASE_EMAIL_LOGIN_LOADING:
			return {
				...state,
				loading: action.payload as boolean,
			}

		default:
			return state
	}
}
