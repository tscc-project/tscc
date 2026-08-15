interface Item14 { id: number; name: string; }
type Key14 = string | number;
export function make14(id: number, name: string): Item14 {
  const value: Item14 = {id, name};
  return value as Item14;
}
export const item14: Item14 = make14(14, "item-14");
