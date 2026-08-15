interface Item24 { id: number; name: string; }
type Key24 = string | number;
export function make24(id: number, name: string): Item24 {
  const value: Item24 = {id, name};
  return value as Item24;
}
export const item24: Item24 = make24(24, "item-24");
