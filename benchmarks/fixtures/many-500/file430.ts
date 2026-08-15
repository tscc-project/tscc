interface Item430 { id: number; name: string; }
type Key430 = string | number;
export function make430(id: number, name: string): Item430 {
  const value: Item430 = {id, name};
  return value as Item430;
}
export const item430: Item430 = make430(430, "item-430");
