import { useEffect, useRef, useState } from 'react'
import { useOutletContext } from 'react-router-dom'
import { Plus } from 'lucide-react'
import { useAppSelector } from '@/hooks/useAppSelector'
import type { DashboardOutletContext } from '@/components/dashboard/Dashboard.component'
import Plants from '@/components/dashboard/plants/Plants.component'

function PlantsContainer() {
	const { setHeaderRight } = useOutletContext<DashboardOutletContext>()
	const devices = useAppSelector(state => state.devices.devices)

	const [pairOpen, setPairOpen] = useState(false)
	const [removeTarget, setRemoveTarget] = useState<{ id: string; nickname: string } | null>(null)
	const [removingId, setRemovingId] = useState<string | null>(null)

	const [lastAddedId, setLastAddedId] = useState<string | null>(null)
	const previousIdsRef = useRef<Set<string> | null>(null)

	useEffect(() => {
		if (!devices) return
		const currentIds = new Set(devices.map(d => d.id))

		if (previousIdsRef.current) {
			const newId = [...currentIds].find(id => !previousIdsRef.current!.has(id))
			if (newId) {
				setLastAddedId(newId)
				const timeout = setTimeout(() => setLastAddedId(null), 3000)
				previousIdsRef.current = currentIds
				return () => clearTimeout(timeout)
			}
		}

		previousIdsRef.current = currentIds
	}, [devices])

	useEffect(() => {
		setHeaderRight(
			<button
				type="button"
				onClick={() => setPairOpen(true)}
				aria-label="Pair a new device"
				className="grid h-11 w-11 place-items-center rounded-full bg-primary text-primary-foreground shadow-soft transition-colors hover:bg-primary-dark active:scale-95"
			>
				<Plus className="h-5 w-5" />
			</button>
		)

		return () => setHeaderRight(null)
	}, [setHeaderRight])

	return (
		<Plants
			devices={devices}
			lastAddedId={lastAddedId}
			removingId={removingId}
			pairOpen={pairOpen}
			removeTarget={removeTarget}
			onPair={() => setPairOpen(true)}
			onPairOpenChange={setPairOpen}
			onRequestRemove={target => setRemoveTarget(target)}
			onRemoveOpenChange={open => {
				if (!open) setRemoveTarget(null)
			}}
			onRemoved={id => {
				setRemovingId(id)
				setTimeout(() => setRemovingId(null), 500)
			}}
		/>
	)
}

export default PlantsContainer
