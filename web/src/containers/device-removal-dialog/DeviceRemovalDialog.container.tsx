import { useNavigate } from 'react-router-dom'
import { useAppDispatch } from '@/hooks/useAppDispatch'
import { useAppSelector } from '@/hooks/useAppSelector'
import RemoveDeviceDialog from '@/components/remove-device-dialog/RemoveDeviceDialog.component'
import { requestRemoveDevice, resetDeviceRemoval } from '@/redux/actions/DeviceRemoval.action'

interface RemoveDeviceDialogContainerProps {
	open: boolean
	onOpenChange: (open: boolean) => void
	deviceId: string
	nickname: string
}

function RemoveDeviceDialogContainer({
	open,
	onOpenChange,
	deviceId,
	nickname,
}: RemoveDeviceDialogContainerProps) {
	const dispatch = useAppDispatch()
	const navigate = useNavigate()
	const { removing, error } = useAppSelector(state => state.deviceRemoval)

	const handleOpenChange = (nextOpen: boolean) => {
		if (!nextOpen) {
			dispatch(resetDeviceRemoval())
		}
		onOpenChange(nextOpen)
	}

	const onConfirm = () => {
		dispatch(
			requestRemoveDevice({
				deviceId,
				onSuccess: () => {
					dispatch(resetDeviceRemoval())
					onOpenChange(false)
					navigate('/dashboard/plants')
				},
			})
		)
	}

	return (
		<RemoveDeviceDialog
			open={open}
			onOpenChange={handleOpenChange}
			nickname={nickname}
			removing={removing}
			error={error}
			onConfirm={onConfirm}
		/>
	)
}

export default RemoveDeviceDialogContainer
