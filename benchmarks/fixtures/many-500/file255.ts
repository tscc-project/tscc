interface Item255 { id: number; name: string; }
type Key255 = string | number;
export function make255(id: number, name: string): Item255 {
  const value: Item255 = {id, name};
  return value as Item255;
}
export const item255: Item255 = make255(255, "item-255");
