interface Item28 { id: number; name: string; }
type Key28 = string | number;
export function make28(id: number, name: string): Item28 {
  const value: Item28 = {id, name};
  return value as Item28;
}
export const item28: Item28 = make28(28, "item-28");
