import type { FormEvent } from 'react'
import { Sprout } from 'lucide-react'
import {
	Dialog,
	DialogContent,
	DialogHeader,
	DialogTitle,
	DialogDescription,
} from '@/components/ui/dialog'
import type { PairingErrorKind } from '@/redux/types/Pairing.type'

export type PairStep = 'code' | 'nickname'

interface PairDeviceDialogProps {
	open: boolean
	onOpenChange: (open: boolean) => void
	step: PairStep
	code: string
	nickname: string
	verifying: boolean
	verifyError: PairingErrorKind | null
	claiming: boolean
	claimError: PairingErrorKind | null
	onCodeChange: (value: string) => void
	onNicknameChange: (value: string) => void
	onSubmitCode: (e: FormEvent) => void
	onSubmitNickname: (e: FormEvent) => void
}

const ERROR_MESSAGES: Record<PairingErrorKind, string> = {
	invalid:
		"That code doesn't look right, or it's expired. Double check the device's screen, or restart the device for a fresh code.",
	connection: "Couldn't reach the server - check your connection and try again.",
}

function PairDeviceDialog({
	open,
	onOpenChange,
	step,
	code,
	nickname,
	verifying,
	verifyError,
	claiming,
	claimError,
	onCodeChange,
	onNicknameChange,
	onSubmitCode,
	onSubmitNickname,
}: PairDeviceDialogProps) {
	return (
		<Dialog open={open} onOpenChange={onOpenChange}>
			<DialogContent className="rounded-3xl">
				{step === 'code' ? (
					<>
						<DialogHeader>
							<DialogTitle>Pair a device</DialogTitle>
							<DialogDescription>
								Enter the 6-character code shown on your L.E.E.F.'s screen.
							</DialogDescription>
						</DialogHeader>

						<form onSubmit={onSubmitCode} className="mt-2 space-y-4">
							<input
								type="text"
								inputMode="text"
								autoCapitalize="characters"
								autoComplete="off"
								autoFocus
								maxLength={6}
								value={code}
								onChange={e =>
									onCodeChange(
										e.target.value.toUpperCase().replace(/[^A-Z0-9]/g, '')
									)
								}
								placeholder="ENTER CODE"
								className="h-16 w-full rounded-2xl border border-input bg-card text-center text-2xl font-black uppercase tracking-[0.5em] text-foreground outline-none transition-shadow placeholder:text-muted-foreground/40 focus:ring-4 focus:ring-primary/20"
							/>

							{verifyError && (
								<p className="text-center text-xs font-semibold text-destructive">
									{ERROR_MESSAGES[verifyError]}
								</p>
							)}

							<button
								type="submit"
								disabled={code.length !== 6 || verifying}
								className="flex h-12 w-full items-center justify-center gap-2 rounded-full bg-primary text-sm font-bold text-primary-foreground shadow-soft transition-colors hover:bg-primary-dark disabled:opacity-60"
							>
								{verifying ? (
									<Sprout className="h-4 w-4 animate-pulse" />
								) : (
									'Continue'
								)}
							</button>
						</form>
					</>
				) : (
					<>
						<DialogHeader>
							<DialogTitle>Name your plant</DialogTitle>
							<DialogDescription>
								Give this device a name you'll recognize - you can change it later.
							</DialogDescription>
						</DialogHeader>

						<form onSubmit={onSubmitNickname} className="mt-2 space-y-4">
							<label className="block">
								<span className="mb-1.5 block text-xs font-bold uppercase tracking-wider text-muted-foreground">
									Nickname
								</span>
								<input
									type="text"
									autoFocus
									autoComplete="off"
									value={nickname}
									onChange={e => onNicknameChange(e.target.value)}
									placeholder="Monstera, Fiddle Leaf, ..."
									className="h-12 w-full rounded-2xl border border-input bg-card px-4 text-sm text-foreground outline-none transition-shadow placeholder:text-muted-foreground focus:ring-4 focus:ring-primary/20"
								/>
							</label>

							{claimError && (
								<p className="text-center text-xs font-semibold text-destructive">
									{ERROR_MESSAGES[claimError]}
								</p>
							)}

							<button
								type="submit"
								disabled={nickname.trim().length === 0 || claiming}
								className="flex h-12 w-full items-center justify-center gap-2 rounded-full bg-primary text-sm font-bold text-primary-foreground shadow-soft transition-colors hover:bg-primary-dark disabled:opacity-60"
							>
								{claiming ? <Sprout className="h-4 w-4 animate-pulse" /> : 'Save'}
							</button>
						</form>
					</>
				)}
			</DialogContent>
		</Dialog>
	)
}

export default PairDeviceDialog
