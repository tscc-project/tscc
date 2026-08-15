interface Item93 { id: number; name: string; }
type Key93 = string | number;
export function make93(id: number, name: string): Item93 {
  const value: Item93 = {id, name};
  return value as Item93;
}
export const item93: Item93 = make93(93, "item-93");
