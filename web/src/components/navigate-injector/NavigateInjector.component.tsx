import { setNavigate } from '@/utils/Navigator'
import { useEffect } from 'react'
import { useNavigate } from 'react-router-dom'

export const NavigateInjector = () => {
	const nav = useNavigate()
	useEffect(() => {
		setNavigate(nav)
	}, [nav])
	return null
}
