interface Item409 { id: number; name: string; }
type Key409 = string | number;
export function make409(id: number, name: string): Item409 {
  const value: Item409 = {id, name};
  return value as Item409;
}
export const item409: Item409 = make409(409, "item-409");
