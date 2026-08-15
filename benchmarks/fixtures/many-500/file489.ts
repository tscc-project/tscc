interface Item489 { id: number; name: string; }
type Key489 = string | number;
export function make489(id: number, name: string): Item489 {
  const value: Item489 = {id, name};
  return value as Item489;
}
export const item489: Item489 = make489(489, "item-489");
