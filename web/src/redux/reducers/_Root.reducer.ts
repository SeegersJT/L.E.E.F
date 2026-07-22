import { combineReducers } from 'redux'
import { notificationReducer } from './Notification.reducer'
import { AuthReducer } from './Authentication.reducer'

export const RootReducer = combineReducers({
	system: combineReducers({
		notification: notificationReducer,
	}),
	auth: AuthReducer,
})
