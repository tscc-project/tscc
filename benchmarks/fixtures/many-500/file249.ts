interface Item249 { id: number; name: string; }
type Key249 = string | number;
export function make249(id: number, name: string): Item249 {
  const value: Item249 = {id, name};
  return value as Item249;
}
export const item249: Item249 = make249(249, "item-249");
