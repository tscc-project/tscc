interface Item412 { id: number; name: string; }
type Key412 = string | number;
export function make412(id: number, name: string): Item412 {
  const value: Item412 = {id, name};
  return value as Item412;
}
export const item412: Item412 = make412(412, "item-412");
