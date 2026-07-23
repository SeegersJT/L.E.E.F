import { useEffect, useState, type FormEvent } from 'react'
import { useAppDispatch } from '@/hooks/useAppDispatch'
import { useAppSelector } from '@/hooks/useAppSelector'
import { requestVerifyCode, requestClaimDevice, resetPairing } from '@/redux/actions/Pairing.action'
import PairDeviceDialog, {
	type PairStep,
} from '@/components/pair-device-dialog/PairDeviceDialog.component'

interface PairDeviceDialogContainerProps {
	open: boolean
	onOpenChange: (open: boolean) => void
}

function PairDeviceDialogContainer({ open, onOpenChange }: PairDeviceDialogContainerProps) {
	const dispatch = useAppDispatch()
	const pairing = useAppSelector(state => state.pairing)

	const [step, setStep] = useState<PairStep>('code')
	const [code, setCode] = useState('')
	const [nickname, setNickname] = useState('')

	useEffect(() => {
		if (pairing.deviceId) {
			setStep('nickname')
		}
	}, [pairing.deviceId])

	const resetLocalState = () => {
		setStep('code')
		setCode('')
		setNickname('')
	}

	const handleOpenChange = (nextOpen: boolean) => {
		if (!nextOpen) {
			resetLocalState()
			dispatch(resetPairing())
		}
		onOpenChange(nextOpen)
	}

	const onSubmitCode = (e: FormEvent) => {
		e.preventDefault()
		if (code.length !== 6) return
		dispatch(requestVerifyCode({ code }))
	}

	const onSubmitNickname = (e: FormEvent) => {
		e.preventDefault()
		if (!pairing.deviceId || nickname.trim().length === 0) return

		dispatch(
			requestClaimDevice({
				code,
				deviceId: pairing.deviceId,
				nickname: nickname.trim(),
				onSuccess: () => {
					resetLocalState()
					dispatch(resetPairing())
					onOpenChange(false)
				},
			})
		)
	}

	return (
		<PairDeviceDialog
			open={open}
			onOpenChange={handleOpenChange}
			step={step}
			code={code}
			nickname={nickname}
			verifying={pairing.verifying}
			verifyError={pairing.verifyError}
			claiming={pairing.claiming}
			claimError={pairing.claimError}
			onCodeChange={setCode}
			onNicknameChange={setNickname}
			onSubmitCode={onSubmitCode}
			onSubmitNickname={onSubmitNickname}
		/>
	)
}

export default PairDeviceDialogContainer
