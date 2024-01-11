#pragma once

template <typename T>
concept deref_able = requires(T t) { *t; };