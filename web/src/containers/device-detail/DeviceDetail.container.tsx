import { useEffect, useState } from 'react'
import { useParams, useOutletContext } from 'react-router-dom'
import { useAppSelector } from '@/hooks/useAppSelector'
import { useAppDispatch } from '@/hooks/useAppDispatch'
import { requestDeviceHistory } from '@/redux/actions/DeviceHistory.action'
import type { DashboardOutletContext } from '@/components/dashboard/Dashboard.component'
import StatusPill from '@/components/status-pill/StatusPill.component'
import { listChannels } from '@/utils/Devices'
import DeviceDetail, {
	RANGE_MS,
	type Range,
} from '@/components/device-detail/DeviceDetails.component'

function DeviceDetailContainer() {
	const { deviceId } = useParams<{ deviceId: string }>()
	const { setHeaderRight, setTitle } = useOutletContext<DashboardOutletContext>()
	const dispatch = useAppDispatch()

	const devices = useAppSelector(state => state.devices.devices)
	const historyState = useAppSelector(state => state.deviceHistory)

	const [range, setRange] = useState<Range>('24h')
	const [sending, setSending] = useState(false)
	const [watering, setWatering] = useState(false)
	const [removeOpen, setRemoveOpen] = useState(false)

	const device = devices?.find(d => d.id === deviceId) ?? null
	const channels = listChannels(device?.status)
	const moisture = channels.find(c => c.type === 'moisture')?.data
	const relay = channels.find(c => c.type === 'relay')?.data

	useEffect(() => {
		setTitle(device?.nickname || 'L.E.E.F. Device')
		setHeaderRight(<StatusPill lastSeen={device?.status?.lastSeen} />)

		return () => {
			setTitle(null)
			setHeaderRight(null)
		}
	}, [device?.nickname, device?.status?.lastSeen, setTitle, setHeaderRight])

	useEffect(() => {
		if (!deviceId) return
		dispatch(requestDeviceHistory({ deviceId, sinceMs: Date.now() - RANGE_MS[range] }))
	}, [deviceId, range, dispatch])

	const onWaterNow = () => {
		if (!relay) return
		setSending(true)
		window.setTimeout(() => {
			setSending(false)
			setWatering(true)
			window.setTimeout(() => setWatering(false), relay.onDurationMs || 5000)
		}, 900)
	}

	return (
		<DeviceDetail
			loading={devices === null}
			device={device}
			moisture={moisture}
			relay={relay}
			sending={sending}
			watering={watering}
			onWaterNow={onWaterNow}
			range={range}
			onRangeChange={setRange}
			historyLoading={historyState.loading}
			historyError={historyState.error}
			moistureHistory={historyState.deviceId === deviceId ? historyState.moisture : []}
			relayHistory={historyState.deviceId === deviceId ? historyState.relay : []}
			removeOpen={removeOpen}
			onRequestRemove={() => setRemoveOpen(true)}
			onRemoveOpenChange={setRemoveOpen}
		/>
	)
}

export default DeviceDetailContainer
