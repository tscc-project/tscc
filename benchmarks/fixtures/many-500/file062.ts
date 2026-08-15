interface Item62 { id: number; name: string; }
type Key62 = string | number;
export function make62(id: number, name: string): Item62 {
  const value: Item62 = {id, name};
  return value as Item62;
}
export const item62: Item62 = make62(62, "item-62");
