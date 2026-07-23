import { Sprout } from 'lucide-react'
import {
	Dialog,
	DialogContent,
	DialogHeader,
	DialogTitle,
	DialogDescription,
	DialogFooter,
} from '@/components/ui/dialog'
import type { DeviceRemovalErrorKind } from '@/redux/types/DeviceRemoval.type'

interface RemoveDeviceDialogProps {
	open: boolean
	onOpenChange: (open: boolean) => void
	nickname: string
	removing: boolean
	error: DeviceRemovalErrorKind | null
	onConfirm: () => void
}

const ERROR_MESSAGES: Record<DeviceRemovalErrorKind, string> = {
	failed: "That device couldn't be removed. It may already be unpaired.",
	connection: "Couldn't reach the server - check your connection and try again.",
}

function RemoveDeviceDialog({
	open,
	onOpenChange,
	nickname,
	removing,
	error,
	onConfirm,
}: RemoveDeviceDialogProps) {
	return (
		<Dialog open={open} onOpenChange={onOpenChange}>
			<DialogContent className="rounded-3xl">
				<DialogHeader>
					<DialogTitle>Remove {nickname}?</DialogTitle>
					<DialogDescription>
						This will disconnect {nickname} from your account. The device will need to
						be paired again with a new code before anyone can use it.
					</DialogDescription>
				</DialogHeader>

				{error && (
					<p className="text-center text-xs font-semibold text-destructive">
						{ERROR_MESSAGES[error]}
					</p>
				)}

				<DialogFooter>
					<button
						type="button"
						onClick={() => onOpenChange(false)}
						disabled={removing}
						className="h-11 rounded-full border border-input px-5 text-sm font-bold text-foreground disabled:opacity-60"
					>
						Cancel
					</button>
					<button
						type="button"
						onClick={onConfirm}
						disabled={removing}
						className="flex h-11 items-center justify-center gap-2 rounded-full bg-destructive px-5 text-sm font-bold text-destructive-foreground disabled:opacity-60"
					>
						{removing ? <Sprout className="h-4 w-4 animate-pulse" /> : 'Remove device'}
					</button>
				</DialogFooter>
			</DialogContent>
		</Dialog>
	)
}

export default RemoveDeviceDialog
