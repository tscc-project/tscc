interface Item76 { id: number; name: string; }
type Key76 = string | number;
export function make76(id: number, name: string): Item76 {
  const value: Item76 = {id, name};
  return value as Item76;
}
export const item76: Item76 = make76(76, "item-76");
