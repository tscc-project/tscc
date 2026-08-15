interface Item31 { id: number; name: string; }
type Key31 = string | number;
export function make31(id: number, name: string): Item31 {
  const value: Item31 = {id, name};
  return value as Item31;
}
export const item31: Item31 = make31(31, "item-31");
