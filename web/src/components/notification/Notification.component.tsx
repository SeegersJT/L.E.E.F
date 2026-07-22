import { useEffect } from 'react'
import { useAppSelector } from '../../hooks/useAppSelector'
import { useAppDispatch } from '../../hooks/useAppDispatch'
import { removeSystemNotification } from '@/redux/actions/Notification.action'
import { toast } from 'sonner'
import { Toaster } from '../ui/sonner'

function Notification() {
	const dispatch = useAppDispatch()
	const { notifications } = useAppSelector(state => state.system.notification)

	useEffect(() => {
		notifications.forEach(notification => {
			const options = {
				id: notification.id,
				description: notification.message,
				duration: notification.duration ?? 5000,
				onDismiss: () => dispatch(removeSystemNotification(notification.id)),
				onAutoClose: () => dispatch(removeSystemNotification(notification.id)),
			}

			switch (notification.type) {
				case 'success':
					toast.success(notification.title, options)
					break
				case 'error':
					toast.error(notification.title, options)
					break
				case 'warning':
					toast.warning(notification.title, options)
					break
				case 'info':
					toast.info(notification.title, options)
					break
			}
		})
	}, [dispatch, notifications])

	return <Toaster position="bottom-right" richColors expand />
}

export default Notification
