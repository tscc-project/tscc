interface Item261 { id: number; name: string; }
type Key261 = string | number;
export function make261(id: number, name: string): Item261 {
  const value: Item261 = {id, name};
  return value as Item261;
}
export const item261: Item261 = make261(261, "item-261");
