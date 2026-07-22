import type { Notification } from '../types/Notification.type'

export const NOTIFICATION_ACTIONS = {
	ADD_SYSTEM_NOTIFICATION: '[NOTIFICATION] - ADD',
	REMOVE_SYSTEM_NOTIFICATION: '[NOTIFICATION] - REMOVE',
} as const

export const addSystemNotification = (payload: Omit<Notification, 'id'>) => ({
	type: NOTIFICATION_ACTIONS.ADD_SYSTEM_NOTIFICATION,
	payload: {
		...payload,
		id: crypto.randomUUID(),
	},
})

export const removeSystemNotification = (id: string) => ({
	type: NOTIFICATION_ACTIONS.REMOVE_SYSTEM_NOTIFICATION,
	payload: { id },
})
