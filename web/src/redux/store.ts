import { createStore, applyMiddleware, compose } from 'redux'
import createSagaMiddleware from 'redux-saga'
import { RootReducer } from './reducers/_Root.reducer'
import { RootSaga } from './sagas/_Root.saga'
import type { RootState } from './types/_Root.type'

const sagaMiddleware = createSagaMiddleware()

const composeEnhancers =
	(typeof window !== 'undefined' &&
		// eslint-disable-next-line @typescript-eslint/no-explicit-any
		(window as any).__REDUX_DEVTOOLS_EXTENSION_COMPOSE__) ||
	compose

export const store = createStore(RootReducer, composeEnhancers(applyMiddleware(sagaMiddleware)))

sagaMiddleware.run(RootSaga)

export type AppDispatch = typeof store.dispatch
export type { RootState }
