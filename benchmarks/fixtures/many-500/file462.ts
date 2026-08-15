interface Item462 { id: number; name: string; }
type Key462 = string | number;
export function make462(id: number, name: string): Item462 {
  const value: Item462 = {id, name};
  return value as Item462;
}
export const item462: Item462 = make462(462, "item-462");
