interface Item427 { id: number; name: string; }
type Key427 = string | number;
export function make427(id: number, name: string): Item427 {
  const value: Item427 = {id, name};
  return value as Item427;
}
export const item427: Item427 = make427(427, "item-427");
