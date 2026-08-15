interface Item498 { id: number; name: string; }
type Key498 = string | number;
export function make498(id: number, name: string): Item498 {
  const value: Item498 = {id, name};
  return value as Item498;
}
export const item498: Item498 = make498(498, "item-498");
