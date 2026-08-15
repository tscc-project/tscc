interface Item192 { id: number; name: string; }
type Key192 = string | number;
export function make192(id: number, name: string): Item192 {
  const value: Item192 = {id, name};
  return value as Item192;
}
export const item192: Item192 = make192(192, "item-192");
