interface Item484 { id: number; name: string; }
type Key484 = string | number;
export function make484(id: number, name: string): Item484 {
  const value: Item484 = {id, name};
  return value as Item484;
}
export const item484: Item484 = make484(484, "item-484");
